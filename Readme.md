## React Native reimplementation in Reason (name needed)

This is a cross platform reimplementation of react native. 
It's written in Reason (a syntax for OCaml) and it's targetting 
native machine code for best performance. It's the best of both
worlds: speed, portability, and great APIs.

There's a catch though: it's just a proof of concept. 

## How to install/contribute (host mac, target ios)

- [install opam](https://opam.ocaml.org/doc/Install.html)
- `opam switch 4.04.0_ios -A 4.04.0`
- ``eval `opam config env```
- Follow installation instructions [here](https://github.com/ocaml-cross/opam-cross-ios)
- `opam pin add jbuilder https://github.com/janestreet/jbuilder.git`
- `opam install jbuilder reason`
- `jbuilder build` - this builds the sources inside `src`
- open `OCaml.xcodeproj`
- Click run

## Acknowledgements

This project is directly based on work by Cristiano Calcagno 
(React Mini) and whitequark (opam-cross-*).
