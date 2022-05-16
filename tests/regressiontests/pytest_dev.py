import pytest
import signal
import subprocess
from pathlib import Path

CWD = Path(__file__).parent.absolute()
PROG_STARTUP_TIME = 0.5


class ProcessWrapper:

    TMP_CONFIG_FILE = CWD / 'default.conf'

    def __init__(self, config_file_data: str):
        self._process = None
        self._cwd = Path(__file__).parent.absolute()
        self._config_file_data = config_file_data

    def make_program(self):
        path = Path(ProcessWrapper.TMP_CONFIG_FILE)
        with open(path, mode='w+') as f:
            f.write(self._config_file_data)

    def set_up(self):
        self._process = subprocess.Popen([
            self._cwd / '../../webserv',
            ProcessWrapper.TMP_CONFIG_FILE
        ])

    def tear_down(self):
        Path(ProcessWrapper.TMP_CONFIG_FILE).unlink(missing_ok=True)
        self._process.send_signal(sig=signal.SIGINT)


@pytest.fixture
def make_webserv():
    """
    Make sure that for each test the `webserv` executable is compiled and
    ready to be run. Return the exit code of the process to the test case.
    :return:
    """
    return subprocess.Popen(['make', '-C', CWD / '../..']).wait()


@pytest.fixture(scope='class', name='config')
def run_webserv(request):
    wrapper = ProcessWrapper(request.param)
    wrapper.make_program()
    wrapper.set_up()
    yield wrapper
    wrapper.tear_down()
