module Brisk = Brisk;

module Cocoa = {
  module Types = CocoaTypes;

  module Application = BriskApplication;
  module Window = BriskWindow;
  module Menu = BriskMenu;

  module BriskView = BriskView;
  module BriskScrollView = BriskScrollView;

  module BriskButton = BriskButton;
  module BriskClickable = BriskClickable;
  module BriskImage = BriskImage;
  module BriskTextView = BriskTextView;
  module GCD = GCD;

  module BriskEffectView = BriskEffectView;
};

module Std = BriskCoreFoundation;

module RunLoop = RunLoop;
module UIRunner = UIRunner;

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
module Clickable = Clickable;
let clickable = Clickable.component;
module EffectView = EffectView;
let effectView = EffectView.component;
module Toolbar = Toolbar;
let toolbar = Toolbar.toolbar;
module SegmentedControl = SegmentedControl;
let segmentedControl = SegmentedControl.segmentedControl;
let activityIndicator = ActivityIndicator.activityIndicator;

module Menu = Menu;
