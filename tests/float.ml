

let () =
  [| 1.; 2.; 3. |] |> Array.iteri (fun i x ->
    print_string (string_of_int i ^ " " ^ string_of_float x ^ "\n")
  )
