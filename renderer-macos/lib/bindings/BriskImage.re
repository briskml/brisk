type t = NSView.t;

type system = [
  | `ActionTemplate
  | `AddTemplate
  | `Advanced
  | `ApplicationIcon
  | `BluetoothTemplate
  | `Bonjour
  | `BookmarksTemplate
  | `Caution
  | `ColorPanel
  | `ColumnViewTemplate
  | `Computer
  | `EnterFullScreenTemplate
  | `Everyone
  | `ExitFullScreenTemplate
  | `FlowViewTemplate
  | `Folder
  | `FolderBurnable
  | `FolderSmart
  | `FollowLinkFreestandingTemplate
  | `FontPanel
  | `GoLeftTemplate
  | `GoRightTemplate
  | `HomeTemplate
  | `IChatTheaterTemplate
  | `IconViewTemplate
  | `Info
  | `InvalidDataFreestandingTemplate
  | `LeftFacingTriangleTemplate
  | `ListViewTemplate
  | `LockLockedTemplate
  | `LockUnlockedTemplate
  | `MenuMixedStateTemplate
  | `MenuOnStateTemplate
  | `MobileMe
  | `MultipleDocuments
  | `Network
  | `PathTemplate
  | `PreferencesGeneral
  | `QuickLookTemplate
  | `RefreshFreestandingTemplate
  | `RefreshTemplate
  | `RemoveTemplate
  | `RevealFreestandingTemplate
  | `RightFacingTriangleTemplate
  | `ShareTemplate
  | `SlideshowTemplate
  | `SmartBadgeTemplate
  | `StatusAvailable
  | `StatusNone
  | `StatusPartiallyAvailable
  | `StatusUnavailable
  | `StopProgressFreestandingTemplate
  | `StopProgressTemplate
  | `TrashEmpty
  | `TrashFull
  | `User
  | `UserAccounts
  | `UserGroup
  | `UserGuest
];

type source = [ | `File(string) | `System(system) | `Bundle(string)];

let stringOfSystem =
  fun
  | `ActionTemplate => "NSActionTemplate"
  | `AddTemplate => "NSAddTemplate"
  | `Advanced => "NSAdvanced"
  | `ApplicationIcon => "NSApplicationIcon"
  | `BluetoothTemplate => "NSBluetoothTemplate"
  | `Bonjour => "NSBonjour"
  | `BookmarksTemplate => "NSBookmarksTemplate"
  | `Caution => "NSCaution"
  | `ColorPanel => "NSColorPanel"
  | `ColumnViewTemplate => "NSColumnViewTemplate"
  | `Computer => "NSComputer"
  | `EnterFullScreenTemplate => "NSEnterFullScreenTemplate"
  | `Everyone => "NSEveryone"
  | `ExitFullScreenTemplate => "NSExitFullScreenTemplate"
  | `FlowViewTemplate => "NSFlowViewTemplate"
  | `Folder => "NSFolder"
  | `FolderBurnable => "NSFolderBurnable"
  | `FolderSmart => "NSFolderSmart"
  | `FollowLinkFreestandingTemplate => "NSFollowLinkFreestandingTemplate"
  | `FontPanel => "NSFontPanel"
  | `GoLeftTemplate => "NSGoLeftTemplate"
  | `GoRightTemplate => "NSGoRightTemplate"
  | `HomeTemplate => "NSHomeTemplate"
  | `IChatTheaterTemplate => "NSIChatTheaterTemplate"
  | `IconViewTemplate => "NSIconViewTemplate"
  | `Info => "NSInfo"
  | `InvalidDataFreestandingTemplate => "NSInvalidDataFreestandingTemplate"
  | `LeftFacingTriangleTemplate => "NSLeftFacingTriangleTemplate"
  | `ListViewTemplate => "NSListViewTemplate"
  | `LockLockedTemplate => "NSLockLockedTemplate"
  | `LockUnlockedTemplate => "NSLockUnlockedTemplate"
  | `MenuMixedStateTemplate => "NSMenuMixedStateTemplate"
  | `MenuOnStateTemplate => "NSMenuOnStateTemplate"
  | `MobileMe => "NSMobileMe"
  | `MultipleDocuments => "NSMultipleDocuments"
  | `Network => "NSNetwork"
  | `PathTemplate => "NSPathTemplate"
  | `PreferencesGeneral => "NSPreferencesGeneral"
  | `QuickLookTemplate => "NSQuickLookTemplate"
  | `RefreshFreestandingTemplate => "NSRefreshFreestandingTemplate"
  | `RefreshTemplate => "NSRefreshTemplate"
  | `RemoveTemplate => "NSRemoveTemplate"
  | `RevealFreestandingTemplate => "NSRevealFreestandingTemplate"
  | `RightFacingTriangleTemplate => "NSRightFacingTriangleTemplate"
  | `ShareTemplate => "NSShareTemplate"
  | `SlideshowTemplate => "NSSlideshowTemplate"
  | `SmartBadgeTemplate => "NSSmartBadgeTemplate"
  | `StatusAvailable => "NSStatusAvailable"
  | `StatusNone => "NSStatusNone"
  | `StatusPartiallyAvailable => "NSStatusPartiallyAvailable"
  | `StatusUnavailable => "NSStatusUnavailable"
  | `StopProgressFreestandingTemplate => "NSStopProgressFreestandingTemplate"
  | `StopProgressTemplate => "NSStopProgressTemplate"
  | `TrashEmpty => "NSTrashEmpty"
  | `TrashFull => "NSTrashFull"
  | `User => "NSUser"
  | `UserAccounts => "NSUserAccounts"
  | `UserGroup => "NSUserGroup"
  | `UserGuest => "NSUserGuest";

[@noalloc] external make: unit => t = "ml_BriskImage_make";

[@noalloc]
external getImageWidth: t => [@unboxed] float =
  "ml_BriskImage_getImageWidth_bc" "ml_BriskImage_getImageWidth";

[@noalloc]
external getImageHeight: t => [@unboxed] float =
  "ml_BriskImage_getImageHeight_bc" "ml_BriskImage_getImageHeight";

[@noalloc]
external setSourceFile: (t, string) => unit = "ml_BriskImage_setSourceFile";

[@noalloc]
external setSourceNamed: (t, string) => unit = "ml_BriskImage_setSourceNamed";

let make = (~source=?, ()) => {
  let img = make();

  switch (source) {
  | Some(src) =>
    switch (src) {
    | `File(source) => setSourceFile(img, source)
    | `System(system) => setSourceNamed(img, stringOfSystem(system))
    | `Bundle(source) => setSourceNamed(img, source)
    }
  | None => ()
  };

  img;
};
