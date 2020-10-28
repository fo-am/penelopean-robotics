#!/bin/bash
cd "$(dirname "$0")"
racket yarnc.scm $1
python yarnasm.py out.asm

