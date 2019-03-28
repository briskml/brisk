open Brisk_macos;
open Brisk.Layout;
open Story;

let commentItem = (~comment: kid, ~children as _: list(unit), ()) => {
  <view>
    <text
      style=[
        color(Color.hex("#000000")),
        kern(0.29),
        font(~size=12., ~weight=`Medium, ()),
        margin4(~top=7., ()),
        padding4(~top=2., ()),
      ]
      value={formatCommentDetails(
        ~username=comment.by.id,
        ~time=comment.time,
      )}
    />
    <text
      style=[
        color(Color.hex("#000000")),
        kern(0.29),
        font(~size=12., ~weight=`Regular, ()),
        margin4(~top=2., ~bottom=7., ()),
      ]
      selectable=true
      html=true
      value={formatComment(comment.text)}
    />
    <view style=[background(Color.hex("#efefef")), height(1.)] />
  </view>;
};

let details = (~story as {story, _}, ~children as _: list(unit), ()) => {
  <scrollView style=Brisk.Layout.[flex(1.), background(Color.hex("#fff"))]>
    <view
      style=[
        flex(1.),
        padding4(~top=9., ~left=13., ~right=13., ()),
        background(Color.hex("#FFFFFF")),
      ]>
      <view style=[]>
        <view style=[flex(1.)]>
          <view style=[padding4(~bottom=5., ())]>
            <text
              style=[
                flex(1.),
                font(~size=17., ~weight=`Semibold, ()),
                kern(0.41),
                lineBreak(`TruncateTail),
                color(Color.hex("#000000")),
              ]
              value={story.title}
            />
          </view>
          <text
            style=[color(Color.hex("#888888")), font(~size=12., ())]
            value={formatDetails(
              ~username=story.by.id,
              ~score=story.score,
              ~commentCount=story.descendants,
              ~url=story.url,
            )}
          />
        </view>
        <view style=[padding4(~top=11., ())]>
          ...{
               let comments =
                 switch (story.kids) {
                 | Some(kids) => Array.to_list(kids)
                 | None => []
                 };

               List.map(comment => <commentItem comment />, comments);
             }
        </view>
      </view>
    </view>
  </scrollView>;
};
