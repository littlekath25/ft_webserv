#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

mkdir -p "$SCRIPT_DIR"/YoupiBanane
touch "$SCRIPT_DIR"/YoupiBanane/youpi.{bad_extension,bla}

mkdir -p "$SCRIPT_DIR"/YoupiBanane/nop
touch "$SCRIPT_DIR"/YoupiBanane/nop/{youpi.bad_extension,other.pouic}

mkdir -p "$SCRIPT_DIR"/YoupiBanane/Yeah
touch "$SCRIPT_DIR"/YoupiBanane/Yeah/not_happy.bad_extension
