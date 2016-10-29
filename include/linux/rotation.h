#ifndef _LINUX_ROTATION_H
#define _LINUX_ROTATION_H

struct dev_rotation {
    int degree;     /* 0 <= degree < 360 */
}; 

struct rotation_range {
    struct dev_rotation rot;  /* device rotation */
    unsigned int degree_range;      /* lock range = rot.degree ± degree_range */
    /* rot.degree - degree_range <= LOCK RANGE <= rot.degree + degree_range */
};

#endif /*_LINUX_ROTATION_H*/
