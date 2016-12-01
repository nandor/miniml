

class obj_base = object(self)
  method private test =
    print_endline "WTF"

  method lol =
    print_endline "Base";
    self#test
end;;


class obj_derived = object(self)
  inherit obj_base as super

  method private test =
    print_endline "derived"

  method lol =
    print_endline "Derived";
    self#test;
    super#test;
    super#lol;
end;;


(new obj_derived)#lol
