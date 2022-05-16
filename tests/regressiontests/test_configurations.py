import time
import pytest
import requests
from pytest_dev import (
    make_webserv,
    run_webserv,
    PROG_STARTUP_TIME
)


@pytest.mark.parametrize(
    'config, port',
    [(
        """
        http {
            server {
                listen          *:8086
                server_name     example.org www.example.org
                client_max_body_size 10
                error_page 400 /full/path/to/file.html
                
                location /one {
                    allowed_methods GET
                }
                
                location /two {
                    allowed_methods POST
                    upload_path     /
                }
                
                location /three {
                    allowed_methods DELETE
                }
                
                location /four {
                    allowed_methods GET POST DELETE
                    index           index.html index.htm
                    upload_path     /upload
                }
            }
        }
        """,
        8086
    )], indirect=['config'], scope='class')
class TestMultipleRoutes:

    def test_statuscodes(self, config, port, make_webserv):
        pass

    def test_methods(self, config, port, make_webserv):
        pass


@pytest.mark.parametrize(
    'config, port',
    [(
        """
        http {
            server {
                listen          *:8087
                location / {
                    allowed_methods GET
                }
            }
            
            server {
                listen          *:8088
                location / {
                    allowed_methods GET
                }
            }
            
            server {
                listen          *:8089
                location / {
                    allowed_methods GET
                }
            }
        }
        """,
        8087
    )], indirect=['config'], scope='class')
class TestMultipleServers:

    def test_statuscodes(self, config, port, make_webserv):
        assert make_webserv == 0
        time.sleep(PROG_STARTUP_TIME)
        start_port = port
        for p in range(start_port, start_port + 3):
            resp = requests.get(url=f'http://localhost:{p}')
            assert resp.status_code == 200
            resp = requests.get(url=f'http://localhost:{p}/somewhere')
            assert resp.status_code == 404

    def test_methods(self, config, port, make_webserv):
        assert make_webserv == 0
        time.sleep(PROG_STARTUP_TIME)
        start_port = port
        for p in range(start_port, start_port + 3):
            resp = requests.post(url=f'http://localhost:{p}')
            assert resp.status_code == 405
            resp = requests.delete(url=f'http://localhost:{p}')
            assert resp.status_code == 405


