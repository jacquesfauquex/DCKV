# Part 6 - processable

The DICOM standard part 6, version docbook, may be easily transformed in resources directly processable by applications.

To do so, we used the xml `<plist>` format using elements `<array>`, `<string>` and `<integer>`.
```
<plist>
    <array>
        <array>
            <integer><!--index-->
        </array>            
        <array>
            <string><!--tag-->
        </array>            
        <array>
            <string><!--description-->
        </array>            
        <array>
            <string><!--keyword-->
        </array>            
        <array>
            <string><!--vr-->
        </array>            
        <array>
            <string><!--vm-->
        </array>            
        <array>
            <string><!--status-->
        </array>            
    </array>
</plist>    

```
The contents of the column "tag" replaces the format "(gggg,eeee)" by the format used in the xml and json representations of DICOM "hhhhhhhh".

We add a first column "index" with the integer value of the tag.

The processing should transform in integer the tag of an attribute to be queried, find this integer within the index column (a B-tree algorithm can be used) and remember the location of it within the "index" array. Find the other properties of the attribute at the same location in the other columns.
