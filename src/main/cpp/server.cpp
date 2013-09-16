#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>


#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

main(int argc, char *argv[]) {
    int port = 3800;
    int threads = 4;
    try {
        po::options_description desc("Janus Image Processing Server options");
        desc.add_options()
            ("help", "produce this help message")
            ("port", po::value<int>(&port)->default_value(3800), "listen port number")
            ("thread", po::value<int>(&threads)->default_value(4), "number of threads")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 1;
        }
    } catch(exception& e) {
        cerr << "error parsing command line: " << e.what() << "\n";
        return 1;
    } catch(...) {
        cerr << "Exception of unknown type in parsing command line!\n";
        return 1;
    }
	return 0;
}
