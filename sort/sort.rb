require 'benchmark'

def random_list(length = 20, limit = 1000000000)
    Array.new(length) { rand(0..limit) }
end

# idk
def blepsort(l)
    # cover edge case that would break things
    return l if l.length <= 1
    sorted = false # is this still needed if we do the sorted_idx thing?
    sorted_idx = l.length - 1
    # not sure what the idiomatic way to edit an array while iterating over it
    # TODO look up loops maybe to see how to do this
    while sorted_idx > 1
        sorted = true
        # don't do the last element cause there's nothing after it
        for i in 0...(sorted_idx)
            sorted_idx = 0
            if l[i] > l[i + 1]
                l[i], l[i + 1] = l[i + 1], l[i]
                sorted_idx = i
            end
        end
        sorted_idx = l.length - 1
    end
    return l
end

def endssort(l)
    i = 0
    j = l.length - 1
    while i < l.length - 1
        while i < j
            if l[i] > l[j]
                l[i], l[j] = l[j], l[i]
            end
            j -= 1
        end
        i += 1
        j = l.length - 1
    end
    return l
end

def quicksort(blep, low, high)
    if low >= 0 and high >= 0 and low < high
        pivot_value = blep[((low + high) / 2).floor]
        l = low - 1
        r = high + 1
        while true
            while true
                l += 1
                if not blep[l] < pivot_value then break end
            end
            while true
                r -= 1
                if not blep[r] > pivot_value then break end
            end
            if l >= r
                new_pivot = r
                break
            end
            tmp = blep[l]
            blep[l] = blep[r]
            blep[r] = tmp
        end

        quicksort(blep, low, new_pivot)
        quicksort(blep, new_pivot + 1, high)
    end
    return blep
end

def iquicksort(blep, low, high)
    s = []
    s.append([low, high])
    # index 0 is low, index 1 is high

    while true
        frame = s.pop
        if frame[0] >= 0 and frame[1] >= 0 and frame[0] < frame[1]
            pivot_value = blep[((frame[0] + frame[1]) / 2).floor]
            l = frame[0] - 1
            r = frame[1] + 1
            while true
                while true
                    l += 1
                    if not blep[l] < pivot_value then break end
                end
                while true
                    r -= 1
                    if not blep[r] > pivot_value then break end
                end
                if l >= r
                    new_pivot = r
                    break
                end
                tmp = blep[l]
                blep[l] = blep[r]
                blep[r] = tmp
            end

            s.append([new_pivot + 1, frame[1]])
            s.append([frame[0], new_pivot])

            # quicksort(blep, low, new_pivot)
            # quicksort(blep, new_pivot + 1, high)
        elsif s.length == 0
            return blep
        end
    end
end
