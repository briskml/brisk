let (fd_out, fd_in) = Unix.pipe();

let bytes_write = Bytes.create(1);
let perform = () => Unix.write(fd_in, bytes_write, 0, 1) |> ignore;

let () = {
  let bytes_read = Bytes.create(1);
  Lwt_engine.on_readable(fd_out, _ =>
    Unix.read(fd_out, bytes_read, 0, 1) |> ignore
  )
  |> ignore;
};
