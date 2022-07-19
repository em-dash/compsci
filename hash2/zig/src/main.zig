const std = @import("std");

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    try stdout.print("hi\n", .{});

    var file = try std.fs.cwd().openFile("../test", .{});
    defer file.close();

    var buf_reader = std.io.bufferedReader(file.reader());
    var another_reader = buf_reader.reader();

    // TODO is ArrayList(u8) guarenteed to be packed?
    // TODO this is not the right allocator but i do not care right now
    var buffer = std.ArrayList(u8).init(std.heap.page_allocator);

    // TODO don't this, do soemthing other than just putting a really big number
    // in max_append_size
    try another_reader.readAllArrayList(&buffer, 18446744073709551615);

    // std.debug.print("{s}", .{buffer.items});
}
