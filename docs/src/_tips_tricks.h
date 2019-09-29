/**
 * \page            page_tips_tricks Tips & Tricks
 * \tableofcontents
 *
 * \section         sect_tip_buff_size Buffer size
 *
 * Buffer size shall always be `1` byte higher than anticipated data size.
 *
 * When application needs buffer for some data block `N` times, it is advised to set buffer size to `1` byte more than `N * block_size` is.
 * This is due to `R` and `W` pointers alignment.
 * For more information, check \ref sect_how_it_works.
 *
 * \include         tip_trick_buff_size.c
 *
 * When executed:
 *
 * \include         tip_trick_buff_size_res.c
 *
 */