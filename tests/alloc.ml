(**************************************************************************)
(*                                                                        *)
(*                                 OCaml                                  *)
(*                                                                        *)
(*              Damien Doligez, projet Para, INRIA Rocquencourt           *)
(*                                                                        *)
(*   Copyright 1996 Institut National de Recherche en Informatique et     *)
(*     en Automatique.                                                    *)
(*                                                                        *)
(*   All rights reserved.  This file is distributed under the terms of    *)
(*   the GNU Lesser General Public License version 2.1, with the          *)
(*   special exception on linking described in the file LICENSE.          *)
(*                                                                        *)
(**************************************************************************)

(* Random allocation test *)

(*
  Allocate arrays of strings, of random sizes in [0..1000[, and put them
  into an array of 32768.  Replace a randomly-selected array with a new
  random-length array.  Reiterate ad infinitum.
*)

let l = 32768;;
let m = 1000;;

let ar = Array.make l "";;

let main () =
  while true do
    for i = 1 to 100000 do
      print_endline (string_of_int i);
      ar.(i mod l) <- String.create i;
    done;
    Gc.full_major ();
    print_newline ();
    Gc.print_stat stdout;
    flush stdout;
  done
;;

main ();;
