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
                    listen          *:8085
                    server_name     example.org www.example.org
                    client_max_body_size 10
                    
                    location /upload {
                        allowed_methods POST
                        upload_path     /
                    }
                    
                    location /delete {
                        allowed_methods DELETE
                    }
                    
                    location / {
                        allowed_methods GET
                        root            /
                        index           index.html index.htm
                    }
                }
            }
            """,
            8085
    )], indirect=['config'], scope='class')
class TestFileUpdates:

    def test_upload_file(self, config, port, make_webserv):
        assert make_webserv == 0
        time.sleep(PROG_STARTUP_TIME)
        data = "some_data".encode('utf-8')
        headers = {'Content-Type': 'text/plain'}
        resp = requests.post(url=f'http://localhost:{port}/upload', data=data, headers=headers)
        assert resp.status_code == 201

    def test_get_file(self, config, port, make_webserv):
        assert make_webserv == 0
        time.sleep(PROG_STARTUP_TIME)
        data = "some_data".encode('utf-8')
        headers = {'Content-Type': 'text/plain'}
        resp = requests.post(url=f'http://localhost:{port}/upload', data=data, headers=headers)
        assert resp.status_code == 201
        resp = requests.get(url=f'http://localhost:{port}/test.plain')
        assert resp.status_code == 200
        assert resp.content == data

    def test_delete_file(self, config, port, make_webserv):
        assert make_webserv == 0
        time.sleep(PROG_STARTUP_TIME)
        data = "some_data".encode('utf-8')
        headers = {'Content-Type': 'text/plain'}
        resp = requests.post(url=f'http://localhost:{port}/upload', data=data, headers=headers)
        assert resp.status_code == 201
        resp = requests.get(url=f'http://localhost:{port}/test.plain')
        assert resp.status_code == 200
        assert resp.content == data
        resp = requests.delete(url=f'http://localhost:{port}/delete/test.plain')
        assert resp.status_code == 200
