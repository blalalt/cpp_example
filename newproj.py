#!usr/bin/python3
import os
import optparse

root_path = os.path.abspath('.')

_cmaketxt_template = """
cmake_minimum_required(VERSION 3.0)
project({project_name})
include_directories(include)
add_executable(main src/main.cc)
"""

_main_func = """
#include <iostream>

int main() {

    return 0;
}
"""

def _gen_proj(project_name: str):
    p_path = os.path.join(root_path, project_name)
    dirs = ['build', 'src', 'include']

    for d in dirs:
        os.makedirs(os.path.join(p_path, d), exist_ok=False)

    files = 'CMakeLists.txt'
    with open(os.path.join(p_path, files), 'w', encoding='utf8') as f:
        s = _cmaketxt_template.format(project_name=project_name)
        f.writelines(s)

    with open(os.path.join(p_path, 'src', 'main.cc'), 'w', encoding='utf8') as f:
        s = _main_func
        f.writelines(s) 


def main():
    parser = optparse.OptionParser()

    parser.add_option("-n", "--name", dest="name", help="Create a c++ project, which includes directories(build, src, include) and\
        CMakeLists.txt")

    options, _ = parser.parse_args() 

    proj_name = options.name # pylint: unused
    _gen_proj(proj_name)


if __name__ == "__main__":
    main()


