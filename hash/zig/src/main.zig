const std = @import("std");
// const Alligator = std.mem.Allocator;

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    try stdout.print("hi\n", .{});

    var file = try std.fs.cwd().openFile("foo.txt", .{});
    defer file.close();

    var buf_reader = std.io.bufferedReader(file.reader());
    var another_reader = buf_reader.reader();

    // TODO is ArrayList(u8) guarenteed to be packed?  maybe PackedIntArray or
    // somethign idk.
    // TODO this is not the right allocator but i do not care right now
    var buffer = std.ArrayListAligned(u8, null).init(std.heap.page_allocator);
    // why do i need ArrayListAligned for readAllArrayList when there's also
    // readAllArrayListAligned?
    // TODO don't this, do soemthing other than just putting the biggest number
    // in max_append_size
    try another_reader.readAllArrayList(&buffer, 18446744073709551615);
}
