#ifndef TEST_ARGS_HPP
#define TEST_ARGS_HPP

#include <cassert>

#include "args/args.hpp"

template <class type>
static void _test_vectors(const std::vector<type> &lhs, const std::vector<type> &rhs) {
    assert(lhs.size() == rhs.size());
    for (std::size_t i = 0, n = lhs.size(); i < n; ++i) {
        if constexpr (std::is_floating_point_v<type>) {
            assert(std::abs(lhs[i] - rhs[i]) < 1e-10);
        } else {
            assert(lhs[i] == rhs[i]);
        }
    }
}

static void _test_properties(args::properties &props) {
    assert(props.size() == 12);
    assert(props.get<std::string>("name3").value() == "line1,line2,line3,line4 ");
    assert(props.get<std::string>("na\\me2").value() == "value\\2");
    assert(props.get<std::string>("server.address").value() == "https://google.com");
    assert(props.get<std::string>("path").value() == "c:\\wikipedia\\templates\\xyz");
    assert(props.get<std::string>("pass\\xwordT").value() == "123456Z");
    assert(
        props.get<std::string>("key with spaces").value() ==
        "It is the value accessible by key \"key with spaces\"."
    );
    assert(props.get<int>("com.test.value").value() == 123);
    assert(props.get<long>("connection.timeout.ms").value() == 3500);
    assert(props.get<std::string>("user").value() == "admin\\ ");
    assert(props.get<std::string>("empty").value() == "");
    assert(props.get<std::string>("\\name1") == "value1    fgfg \\xyz");
    assert(std::abs(props.get<double>("server.timeout").value() - 5.45) < 1e-10);
}

void test_value() {
    assert(args::value{"value"}.get<std::string>().value() == "value");
    assert(args::value{""}.set("X").get<std::string>().value() == "X");
    assert(args::value{""}.add("Y").get<std::string>().value() == "Y");
    assert(args::value{"123"}.get<int>().value() == 123);
    assert(std::abs(args::value{"123.456"}.get<double>().value() - 123.456) < 1e-10);
    assert(args::value{"tRuE"}.get<bool>().value() == true);
    assert(args::value{"1"}.get<bool>().value() == true);
    assert(args::value{"0"}.get<bool>().value() == false);
    assert(args::value{"text"}.get<bool>().value() == false);
    assert(args::value{"11.22"}.get_values().value().size() == 1);
    assert(std::abs(args::value{"11.22"}.get_values().value()[0].get<float>().value() - 11.22f) < 1e-10f);
    std::string value;
    value.append("22");
    value.push_back(0);
    value.append("33");
    value.push_back(0);
    value.append("44");
    _test_vectors<int>(
        args::value{std::move(value)}.get_values<int>().value(),
        std::vector<int>{22, 33, 44}
    );
}

void test_options() {
    const char *argv[] = {
        "executable", "arg1", "arg2",
        "--help",
        "--log=0",
        "-v true",
        "-a 10",
        "--type=int",
        "--set", "set1", "set2", "set3",
        "-b20",
        "--title=text with spaces",
        "-b", "30",
        "-c", "1.2", "2.3", "3.4",
        "--", "arg3", "arg4"
    };
    args::options opts{sizeof(argv) / sizeof(argv[0]), argv};
    assert(opts.get_application_name() == "executable");
    assert(opts.get_positional_size() == 4);
    assert(opts.get_named_size() == 9);
    assert(opts.get<std::string>(0).value() == "arg1");
    assert(opts.get<std::string>(1).value() == "arg2");
    assert(opts.get<std::string>(2).value() == "arg3");
    assert(opts.get<std::string>(3).value() == "arg4");
    assert(opts.get<bool>("help").value() == true);
    assert(opts.get<bool>("log").value() == false);
    assert(opts.get<bool>("v").value() == true);
    assert(opts.get<int>("a").value() == 10);
    assert(opts.get<std::string>("type").value() == "int");
    _test_vectors<std::string>(
        opts.get("set").value().get_values<std::string>().value(),
        std::vector<std::string>{"set1", "set2", "set3"}
    );
    _test_vectors<int>(
        opts.get("b").value().get_values<int>().value(),
        std::vector<int>{20, 30}
    );
    assert(opts.get<std::string>("title").value() == "text with spaces");
    _test_vectors<double>(
        opts.get("c").value().get_values<double>().value(),
        std::vector<double>{1.2, 2.3, 3.4}
    );

    opts.add_option(
        "help,h", "Show help message", false
    ).add_option(
        "title", "Desired title", true
    ).add_option(
        "a", "First coefficient", true
    );
    std::ostringstream out;
    assert(opts.validate(out) == true);
    assert(out.str() == "");
    opts.add_option(
        "extra,x", "Non existent parameter", true
    ).add_option(
        "r", "Required parameter", true
    );
    assert(opts.validate(out) == false);
    assert(
        out.str() ==
        "Required: --extra,x Non existent parameter\n"
        "Required: -r Required parameter\n"
    );
    out.str("");
    opts.show_options(out);
    assert(
        out.str() ==
        "executable --[help,h] --<title> -<a> --<extra,x> -<r>\n"
        "--help,h Show help message [optional]\n"
        "--title Desired title <required>\n"
        "-a First coefficient <required>\n"
        "--extra,x Non existent parameter <required>\n"
        "-r Required parameter <required>\n"
    );
}

void test_properties() {
    std::stringstream input;
    input << "  \t  name3  \t= \t line1,\\\r\r      \t  line2,\\\rline3,\\\n     line4 \r\n" << std::endl;
    input << "#comment1=value1" << std::endl;
    input << "!comment2=value2" << std::endl;
    input << " #comment3=value3" << std::endl;
    input << " \t !comment4=value4" << std::endl;
    input << "na\\me2:value\\2" << std::endl;
    input << "server.address : https://www.google.com" << std::endl;
    input << "path=c:\\wikipedia\\templates\\xyz" << std::endl;
    input << "pass\\xword\\x54=123456\x5A" << std::endl;
    input << "key\\ with\\ spaces=It is the value accessible by key \"key with spaces\"." << std::endl;
    input << "com.test.value=123" << std::endl;
    input << "connection.timeout.ms=3500" << std::endl;
    input << "user=admin\\ " << std::endl;
    input << "empty=" << std::endl;
    input << "\\name1=value1    fgfg \\xyz" << std::endl;
    input << "#\n";
    input << "!\r";

    args::properties props{input};
    props.set("server.address", "https://google.com");
    props.set("server.timeout", "5.45");

    _test_properties(props);

    std::stringstream output;
    assert(props.save(output));

    args::properties props_for_load;
    assert(props_for_load.load(output));

    _test_properties(props_for_load);
}

#endif
