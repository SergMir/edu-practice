#!/bin/sh

set -x

gdbserver --once :2345 $1
