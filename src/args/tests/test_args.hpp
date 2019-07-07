#ifndef TEST_ARGS_HPP
#define TEST_ARGS_HPP

#include "args/args.hpp"

template <class type>
static void test_vectors(const std::vector<type> &lhs, const std::vector<type> &rhs) {
    REQUIRE(lhs.size() == rhs.size());
    for (std::size_t i = 0, n = lhs.size(); i < n; ++i) {
        REQUIRE(lhs[i] == rhs[i]);
    }
}

TEST_CASE("Test value", "[args]") {
    REQUIRE(args::value{"value"}.get<std::string>().value() == "value");
    REQUIRE(args::value{""}.set("X").get<std::string>().value() == "X");
    REQUIRE(args::value{""}.add("Y").get<std::string>().value() == "Y");
    REQUIRE(args::value{"123"}.get<int>().value() == 123);
    REQUIRE(args::value{"123.456"}.get<double>().value() == 123.456);
    REQUIRE(args::value{"tRuE"}.get<bool>().value() == true);
    REQUIRE(args::value{"1"}.get<bool>().value() == true);
    REQUIRE(args::value{"0"}.get<bool>().value() == false);
    REQUIRE(args::value{"text"}.get<bool>().value() == false);
    REQUIRE(args::value{"11.22"}.get_values().value().size() == 1);
    REQUIRE(args::value{"11.22"}.get_values().value()[0].get<float>().value() == 11.22f);
    std::string value;
    value.append("22");
    value.push_back(0);
    value.append("33");
    value.push_back(0);
    value.append("44");
    test_vectors<int>(
        args::value{std::move(value)}.get_values<int>().value(),
        std::vector<int>{22, 33, 44}
    );
}

TEST_CASE("Test options", "[args]") {
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
    REQUIRE(opts.get_application_name() == "executable");
    REQUIRE(opts.get_positional_size() == 4);
    REQUIRE(opts.get_named_size() == 9);
    REQUIRE(opts.get<std::string>(0).value() == "arg1");
    REQUIRE(opts.get<std::string>(1).value() == "arg2");
    REQUIRE(opts.get<std::string>(2).value() == "arg3");
    REQUIRE(opts.get<std::string>(3).value() == "arg4");
    REQUIRE(opts.get<bool>("help").value() == true);
    REQUIRE(opts.get<bool>("log").value() == false);
    REQUIRE(opts.get<bool>("v").value() == true);
    REQUIRE(opts.get<int>("a").value() == 10);
    REQUIRE(opts.get<std::string>("type").value() == "int");
    test_vectors<std::string>(
        opts.get("set").value().get_values<std::string>().value(),
        std::vector<std::string>{"set1", "set2", "set3"}
    );
    test_vectors<int>(
        opts.get("b").value().get_values<int>().value(),
        std::vector<int>{20, 30}
    );
    REQUIRE(opts.get<std::string>("title").value() == "text with spaces");
    test_vectors<double>(
        opts.get("c").value().get_values<double>().value(),
        std::vector<double>{1.2, 2.3, 3.4}
    );
}

static void test_properties(args::properties &props) {
    REQUIRE(props.size() == 12);
    REQUIRE(props.get<std::string>("name3").value() == "line1,line2,line3,line4 ");
    REQUIRE(props.get<std::string>("na\\me2").value() == "value\\2");
    REQUIRE(props.get<std::string>("server.address").value() == "https://google.com");
    REQUIRE(props.get<std::string>("path").value() == "c:\\wikipedia\\templates\\xyz");
    REQUIRE(props.get<std::string>("pass\\xwordT").value() == "123456Z");
    REQUIRE(props.get<std::string>("key with spaces").value() == "It is the value accessible by key \"key with spaces\".");
    REQUIRE(props.get<int>("com.test.value").value() == 123);
    REQUIRE(props.get<long>("connection.timeout.ms").value() == 3500);
    REQUIRE(props.get<std::string>("user").value() == "admin\\ ");
    REQUIRE(props.get<std::string>("empty").value() == "");
    REQUIRE(props.get<std::string>("\\name1") == "value1    fgfg \\xyz");
    REQUIRE(props.get<double>("server.timeout").value() == 5.45);
}

TEST_CASE("Test properties", "[args]") {
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

    test_properties(props);

    std::stringstream output;
    REQUIRE(props.save(output));

    args::properties props_for_load;
    REQUIRE(props_for_load.load(output));

    test_properties(props_for_load);
}

#endif
