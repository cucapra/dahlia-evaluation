#!/usr/bin/env python3

import re
import json
import argparse
import common
import sys
import os
import itertools
import copy
import shutil

TEMPLATE = 'template.json'

def get_assignment(template, indices):
    """Get an assignment using the indices list. The length of the list
    should correspond to the number of template parameters.
    """
    out = copy.deepcopy(template)
    idxs = list(indices)
    for file, pats in template.items():
        for pat, assigns in pats.items():
            out[file][pat] = assigns[idxs.pop(0)]

    return out


def generate_all_assignments(template):
    """Generate assignments indices for all parameters for every file.
    The index at a position corresponds to the index into the list for a parameter
    in the template
    """

    def get_index_lengths(template):
        """For every parameter mapping, calculate the length of arrays.
        """
        index_map = []
        for file, pats in template.items():
            for pat, assigns in pats.items():
                index_map.append(len(assigns))

        return index_map

    def index_combs(index_map):
        """Generate a list of all possible index combinations given `index_map`
        which contains the length of an index at at index's position.
        Ex:
            index_map = [2, 2]
            out = [(0, 0), (0, 1), (1, 0), (1, 1)]
        """
        choices = [list(range(i)) for i in index_map]
        return itertools.product(*choices)


    return index_combs(get_index_lengths(template))


def gen_updated_file(filename, assignment):
    """Update the contents of `filename` with the assignments in `assignment`.
    If a key in `assignment` does not exist in `filename`, throw an error.
    """
    contents = None
    with open(filename, 'r') as f:
        contents = f.read()

    for key, assign in assignment.items():
        key_pattern = "::{}::".format(key)
        assert re.search(key_pattern, contents), "{} missing in {}".format(key_pattern, filename)
        contents = contents.replace(key_pattern, str(assign))

    return contents


def gen_updated_directory(bench, template, assignment, force):
    """
    """
    dst = bench + '-' + '-'.join(str(e) for e in assignment)
    common.logging.info('Creating {}.'.format(dst))
    # chdir to parent of bench
    with common.chdir(os.path.join(bench, "../")):
        if force:
            shutil.rmtree(dst, True)

        shutil.copytree(bench, dst, symlinks=True)

    with common.chdir(dst):
        # Get concrete assignments
        assign = get_assignment(template, assignment)

        # Add template.json
        with open(TEMPLATE, 'w') as t:
            json.dump(assign, t, indent=2)

        # Update template files
        for file, params in assign.items():
            assert os.path.exists(file), "{} is mentioned in {} but missing from {}.".format(file, TEMPLATE, bench)
            new_contents = gen_updated_file(file, params)
            with open(file, 'w') as f:
                f.write(new_contents)

def gen_dse(bench, force):
    """Generate copies of benchmark by varying parameters specified in template.json. The constants must be surrounded as ::CONST:: to distinguish them from #define constants.
    """

    try:
        bench_abs = os.path.abspath(bench)
        template_path = os.path.join(bench_abs, TEMPLATE)
        assert os.path.exists(template_path), "Required file {} missing".format(template_path)

        with open(template_path) as f:
            template = json.load(f)

        for assign in generate_all_assignments(template):
            gen_updated_directory(bench_abs, template, assign, force)

    except AssertionError as err:
        common.logging.error(err)

    except FileExistsError as err:
        common.logging.error(str(err) + ". Use --force to remove existing folders.")


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="Generate DSE folders for a given benchmark.")

    parser.add_argument('-f', '--force',
                        help='Remove existing directories.',
                        action='store_true', dest='force')

    parser.add_argument("bench", type=str)

    opts = parser.parse_args()

    common.logging_setup()

    gen_dse(opts.bench, opts.force)
