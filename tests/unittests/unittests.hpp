#pragma once

#include <curl/curl.h>
#include <gtest/gtest.h>

#include <Webserv.hpp>

#include <future>

#define SOCKET_SETUP_TIME 500000

size_t  writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data);

int     sendEmptyRequest(const char *url, std::string* data);
int     sendBigRequest(const char *url, std::string* data);
