module Brisk = Brisk;

module Cocoa = {
  module Types = CocoaTypes;

  module Application = BriskApplication;
  module Window = BriskWindow;
  module Menu = BriskMenu;

  module BriskView = BriskView;

  module BriskButton = BriskButton;
  module BriskImage = BriskImage;
  module BriskTextView = BriskTextView;
  module GCD = GCD;

  module BriskEffectView = BriskEffectView;
};

/* Components */
module View = View;
let view = View.component;
module ScrollView = ScrollView;
let scrollView = ScrollView.component;
module Text = Text;
let text = Text.component;
module Image = Image;
let image = Image.component;
module Button = Button;
let button = Button.component;
module EffectView = EffectView;
let effectView = EffectView.component;
