
let _ =
  let x = ref 0 in
  for i = 0 to 10 do
    x := !x + 1;
  done;
  print_endline (string_of_int !x);
  ;;
