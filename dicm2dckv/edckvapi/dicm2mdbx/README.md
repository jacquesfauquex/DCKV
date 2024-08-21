MDBX asynchronical append
======================
 
```
   LIBMDBX_API int mdbx_put   (   MDBX_txn *    txn,
    MDBX_dbi    dbi,
    const MDBX_val *    key,
    MDBX_val *    data,
    MDBX_put_flags_t    flags
    )
```

## flags

   MDBX_RESERVE
    Reserve space for data of the given size, but don't copy the given data. Instead, return a pointer to the reserved space, which the caller can fill in later - before the next update operation or the transaction ends. This saves an extra memcpy if the data is being generated later. MDBX does nothing else with this memory, the caller is expected to modify all of the space requested.
   MDBX_APPEND
    Append the given key/data pair to the end of the database. This option allows fast bulk loading when keys are already known to be in the correct order.

## key value

```
    struct MDBX_val {
      void *iov_base; // pointer to some data
      size_t iov_len; // the length of data in bytes
    };
```
