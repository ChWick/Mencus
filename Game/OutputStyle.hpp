#ifndef _OUTPUT_STYLE_HPP_
#define _OUTPUT_STYLE_HPP_

//! Enumeration for defining the style of a output to file
/**
 * \brief When saveing e.g. the map, this can be due to saving as snapshot or if the user wants to
 *        store the file. A snapshot needs all information while a map only needs the minimal information
 *        for reloading in the initial state
 */
enum EOutputStyle {
  OS_FULL,         //!< Full output to file, as needed for snapshots
  OS_MINIMAL,      //!< Minimal output to file as needed for saving maps
};

#endif
