# Brisk

[![Build Status](https://dev.azure.com/briskml/brisk/_apis/build/status/Azure%20DevOps)](https://dev.azure.com/briskml/brisk/_build/latest?definitionId=1)

Cross-platform set of tools for building native UIs with Reason/OCaml.

The goal of the project is to help developers create desktop and mobile apps with the ease of React Native and performance of native machine code. It's the best of both worlds: speed and portability, simplicity and great APIs.

There's a catch, though: it's _Work In Progress_.

## Getting started

This repository contains platform-agnostic core library and platform-specific renderer bindings.

We will provide tools for creating apps and trying out brisk in future, but in the meantime you can explore the concepts, build, and test the code.

To set up the workflow and pull all dependencies, simply run the following command from the project root:

```
% esy install
```

> To install `esy` itself, simply run `npm -g i esy@preview`.

To build the project, run:

```
% esy build
```

## Contributing [TBD]

Run `esy jbuilder runtest` to run the test suites.

> If you're using `yarn`, running `yarn test` will give you nice colorized output.

## Acknowledgements

This project is directly based on the work of [@cristianoc](https://github.com/cristianoc) (`React Mini`).
