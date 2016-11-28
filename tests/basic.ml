
exception Test of int

let _ =
  let x = ref 0 in
  try
    for i = 0 to 10 do
      x := !x + 1;
      if i = 2 then raise (Test i);
    done;
  with Test i ->
    print_endline (string_of_int i);
  print_endline (string_of_int !x);
  ;;
