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
                listen          *:8082
                server_name     example.org www.example.org
                client_max_body_size 10
                error_page 400 /full/path/to/file.html
                location / {
                    allowed_methods GET POST DELETE
                    root            /
                    autoindex       on
                    index           index.html index.htm
                    cgi_extensions  .php .py
                    upload_path     /
                }
            }
        }
        """,
        8082
    )], indirect=['config'], scope='class')
class TestStatusCode2xx:

    def test_statuscode_200(self, config, port, make_webserv):
        assert make_webserv == 0
        time.sleep(PROG_STARTUP_TIME)
        resp = requests.get(url=f'http://localhost:{port}')
        assert resp.status_code == 200


@pytest.mark.parametrize(
    'config, port',
    [(
        """
        http {
            server {
                listen          *:8083
                server_name     example.org www.example.org
                client_max_body_size 10
                error_page 400 /full/path/to/file.html
                location / {
                    allowed_methods GET POST DELETE
                    root            /
                    autoindex       on
                    index           index.html index.htm
                    cgi_extensions  .php .py
                    upload_path     ./
                    return          301 www.google.com
                }
            }
        }
        """,
        8083
    )], indirect=['config'], scope='class')
class TestStatusCode3xx:

    def test_statuscode_301(self, config, port, make_webserv):
        assert make_webserv == 0
        time.sleep(PROG_STARTUP_TIME)
        resp = requests.get(url=f'http://localhost:{port}', allow_redirects=False)
        assert resp.status_code == 301
        assert resp.headers.get('Location', None) is not None


@pytest.mark.parametrize(
    'config, port',
    [(
            """
            http {
                server {
                    listen          *:8084
                    server_name     example.org www.example.org
                    client_max_body_size 10
                    error_page 400 /full/path/to/file.html
                    location /unknown {
                        allowed_methods GET POST DELETE
                        root            /data/w3
                        autoindex       on
                        index           index.html index.htm
                        cgi_extensions  .php .py
                        upload_path     ./
                    }
                }
            }
            """,
            8084
    )], indirect=['config'], scope='class')
class TestStatusCode4xx:

    def test_statuscode_404(self, config, port, make_webserv):
        assert make_webserv == 0
        time.sleep(PROG_STARTUP_TIME)
        resp = requests.get(url=f'http://localhost:{port}')
        assert resp.status_code == 404
