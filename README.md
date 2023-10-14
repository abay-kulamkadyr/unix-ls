# Custom Implementation of the Unix `ls` Command

## Table of Contents

- [Custom Implementation of the Unix `ls` Command](#custom-implementation-of-the-unix-ls-command)
  - [Table of Contents](#table-of-contents)
  - [Description](#description)
  - [Features](#features)
  - [Usage](#usage)
  - [Installation](#installation)

## Description

This project is a custom implementation of the Unix `ls` command for a Linux system. The program supports the following options: `-R`, `-l`, and `-i`, all of which can be used individually or combined together.

## Features

The program supports the following options:

- `-l`: List with long format - shows permissions.
- `-R`: Displays subdirectories.
- `-i`: Lists a file's inode index number.

## Usage

```bash
./UnixLs [-R] OR [-l] OR [-i] OR [FILENAME] OR [DIRNAME]
```

## Installation

This project does not have any external dependencies; the entire program is contained within a single file. Makefile is provided

```bash
# Installation instructions
git clone git@github.com:abay-kulamkadyr/unix-ls.git
make
```