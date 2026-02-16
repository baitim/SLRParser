<h1 align="center">SLR Parser</h1>

## Description

 Implementation of the SLR Parser.<br>

## How to integrate
 
 use [storage](https://github.com/baitim/ConanPackages), project = "slr_parser", version = "1.0", user = "baitim"

## How to run

1. Clone <br>
    <code>git clone https://github.com/baitim/SLRParser.git</code>

2. Go to folder <br>
    <code>cd SLRParser</code>

3. Prepare conan <br>
    <code>uv sync --group dev; source .venv/bin/activate</code><br>
    <code>conan profile detect --force</code>

4. Init dependencies <br>
    <code>conan install . --build=missing -s build_type=Release</code>

5. Build <br>
    <code>cmake --preset release; cmake --build build/Release</code>

6. Run <br>
    <code>./build/Release/SLRParser/slr-parser \<program\></code>

## How to test

* Run testing <br>
    <code>ctest --test-dir build/Release --output-on-failure</code>

<p align="center"><img src="https://github.com/baitim/SLRParser/blob/main/images/cat.gif" width="50%"></p>

## Support
**This project is created by [baitim](https://t.me/bai_tim)**