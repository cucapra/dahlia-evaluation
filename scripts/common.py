import logging
import sys
import os

from contextlib import contextmanager

@contextmanager
def chdir(path):
    """Temporarily change the working directory (then change back).
    """
    old_dir = os.getcwd()
    os.chdir(path)
    yield
    os.chdir(old_dir)

def logging_setup():
    # Color for warning and error mesages
    logging.addLevelName(
        logging.WARNING,
        "\033[1;33m%s\033[1;0m" % logging.getLevelName(logging.WARNING))
    logging.addLevelName(
        logging.ERROR,
        "\033[1;31m%s\033[1;0m" % logging.getLevelName(logging.ERROR))

    logging.basicConfig(
        format='%(levelname)s: %(message)s',
        stream=sys.stderr,
        level=logging.DEBUG)
