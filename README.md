# Brisk

[![Build Status](https://dev.azure.com/briskml/brisk/_apis/build/status/Azure%20DevOps)](https://dev.azure.com/briskml/brisk/_build/latest?definitionId=1)

A cross-platform set of tools for building native UIs with Reason/OCaml.

The goal of the project is to allow developers efficiently create fast, native apps for all major platforms. It's the best of both worlds: speed and portability, simplicity and great APIs.

:construction: **NOTE**: Brisk is a _Work In Progress_. :construction:

- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [Contributing](#contributing)
- [Community](#community)

---

## Requirements

Our development workflow is managed with [esy](https://esy.sh/). It's an incredibly fast package manager that provides reproducible, sandboxed environment for your projects and caches builds.

```
npm -g i esy@latest
```

> We're open to contributions for supporting `opam` workflow

### macOS

##### Xcode

In order to build the OSX binary you will need to install [`Xcode`](https://developer.apple.com/xcode/), as well as `Command Line Tools`:

```
xcode-select --install
```

### Other Platforms

> We're looking for contributors who would help us kickstart `Windows` and `Android` renderers.

## Getting Started

### Examples

This repository contains the core tooling, platform-specific renderers, and examples.
To view examples, clone the repository and open an example project.

```
git clone git@github.com:briskml/brisk.git
cd brisk/examples/components-macos
```

Building and running the example is a breeze:

```
esy
esy run
```

### Motivation

We consider UI development an unsolved problem which is at the core of many inefficiencies both for creators and users. As a result, businesses lose money, hobbyists can't release their side projects, and users suffer from poor quality.

We've looked at React Native, Flutter, platform specific libraries, and hybrid frameworks. All of them have different trade-offs: you either give up runtime performance, developer efficiency, or quality. What's more, none of those solutions make it really easy and fast to build native UI apps. React (Native) is the closest one, but as good as it is, JavaScript constrains both runtime performance and developer productivity. React.js itself, our inspiration, is a great framework and Reason gives us tools to improve upon it.

With Brisk, we want to make building great performant apps quick and painless.

### Design Decisions

We're building Brisk in Reason, a language with great performance characteristics - low memory footprint, near instant startup, and very fast execution.
More importantly, it allows for great expressive APIs and has robust type system features, such as: _Algebraic Data Types_, _Modules_, and _Pattern matching_ to name a few. ([More about Reason](https://reasonml.github.io))

All the layout and UI interactions happen on the main thread, leaving all IO/Networking on a secondary, background thread.

We made a deliberate choice to use native widgets to deliver the smoothest and platform-like experience. The `macOS` and `iOS` renderers leverage `Cocoa`; `Windows` renderer will build upon `WPF` (or `UWP`, _help us make the choice_); `Android` renderer will interface with `Android SDK`, etc.

This will provide great performance and allow for using both the native system controls, and implementing identical cross-platform components.

If you'd rather have a single cross-platform codebase similar to Electron apps, our sister project [Revery](https://github.com/revery-ui/revery)'s contributors took it upon themselves to rebuild the whole UI infrastructure from scratch to achieve the same UI on all platforms (think `flutter`).

Fortunately for everyone, Brisk's core has been factored out into [brisk-reconciler](https://github.com/briskml/brisk-reconciler) - a separate framework that now powers both projects. In the future, we expect developers to be able to seamlessly switch between Revery and Brisk for different parts of their application. 🤯

## Contributing

Follow the instructions for the editor plugins [here](https://reasonml.github.io/docs/en/editor-plugins#native-project-development-community-supported).

To set up a development workflow, simply run this from the project root:

```
esy
```

It will install all dependencies and build the project.

To execute the test suites for `macOS` renderer, use:

```
esy test:mac
```

Check out the [Good First Issues](https://github.com/briskml/brisk/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22) list and don't forget to join our [Discord server](https://discord.gg/TAhzPfF) if you have any questions.

## Community

- Join our common `Reason Native UI` Discord server: https://discord.gg/TAhzPfF
