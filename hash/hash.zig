const std = @import("std");

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    try stdout.print("hi\n", .{});

    var file = try std.fs.cwd().openFile("foo.txt", .{});
    defer file.close(); // TODO what is this lol

    var buf_reader = io.bufferedReader
}
