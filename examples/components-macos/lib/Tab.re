type t =
  | Welcome
  | Views
  | Buttons
  | Text
  | Image
  | System;

let label =
  fun
  | Welcome => "Welcome"
  | Views => "Views"
  | Buttons => "Buttons"
  | Text => "Text"
  | Image => "Image"
  | System => "System";
