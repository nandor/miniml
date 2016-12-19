
type record = {
  mutable x: float;
  mutable y: float;
}


let () =
  let s = { x = 4.0; y = 4.5 } in
  s.x <- 10.5;
  s.y <- 20.0;
  print_endline (string_of_float s.x);
  let x = [| 1.; 2.; 3. |] in
  x.(0) <- 3.5;
  x.(1) <- 4.5;
  x.(2) <- 5.5;
  Array.iteri (fun i x ->
    print_string (string_of_int i ^ " " ^ string_of_float x ^ "\n")
  ) x
