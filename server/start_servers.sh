#!/bin/bash
./CidDirectoryServer &
./ContentServer2 &
sleep 30
./ContentServer3 &

