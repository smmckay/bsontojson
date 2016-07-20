# bsontojson
Convert BSON streams of any size to real JSON

Existing tools for BSON manipulation will try to read entire documents, which is a problem if you have very large documents. You might not get standard-compliant JSON out either. If you're not doing any transforms and don't care about round-tripping your data back to MongoDB, it's straightforward to convert BSON documents to a standard-compliant JSON document.

## Building
    make
You do need a C99 compiler with 64-bit doubles. This is approximately everyone, aside from Visual Studio. I expect MinGW would work for Windows builds, although that's not tested.

## Running
    bsontojson < my_collection.bson > my_collection.json
    
## Limitations

* Weird Mongo types are supported with varying levels of fidelity. Most have some kind of representation in the output of `bsontojson`. Binaries are converted to a placeholder string.
* There is essentially zero error checking. If your BSON is corrupt or doesn't follow [the spec](http://bsonspec.org/spec.html), expect lots of garbage instead of JSON.

## Contributing

Fork and send a pull request! Anything that doesn't break strict JSON conformance is likely to be accepted.
