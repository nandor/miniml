


let _ =
  try
    print_endline (string_of_int (1 / 0))
  with _ ->
    print_endline "Caught";
    print_endline (string_of_int (1 mod 0));
