#!/bin/bash
valgrind \
--tool=callgrind \
--combine-dumps=yes \
--instr-atstart=no \
cmsRun $1

