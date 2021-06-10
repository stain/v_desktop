# v_desktop

This repository is a snapshot of [Netlab's Voyager Desktop](http://trac.netlabs.org/v_desktop) source code and is **no longer maintained**.

## License/copyright

License and copyright was not explicit across the original source repository, but where in-line, is indicated as Dual-license CDDL 1.0/LGPL 2.1, e.g.:

```
/* ***** BEGIN LICENSE BLOCK *****
* Version: CDDL 1.0/LGPL 2.1
*
* The contents of this file are subject to the COMMON DEVELOPMENT AND
* DISTRIBUTION LICENSE (CDDL) Version 1.0 (the "License"); you may not use
* this file except in compliance with the License. You may obtain a copy of
* the License at http://www.sun.com/cddl/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is "NOM" Netlabs Object Model
*
* The Initial Developer of the Original Code is
* netlabs.org: Chris Wohlgemuth <cinc-ml@netlabs.org>.
* Portions created by the Initial Developer are Copyright (C) 2005-2007
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*
* Alternatively, the contents of this file may be used under the terms of
* the GNU Lesser General Public License Version 2.1 (the "LGPL"), in which
* case the provisions of the LGPL are applicable instead of those above. If
* you wish to allow use of your version of this file only under the terms of
* the LGPL, and not to allow others to use your version of this file under
* the terms of the CDDL, indicate your decision by deleting the provisions
* above and replace them with the notice and other provisions required by the
* LGPL. If you do not delete the provisions above, a recipient may use your
* version of this file under the terms of any one of the CDDL or the LGPL.
*
* ***** END LICENSE BLOCK ***** */
```

Documentation adapted from <http://trac.netlabs.org/v_desktop>:

# Voyager Desktop

The Voyager Desktop is an object oriented desktop which tries to resemble the unique features of
the WPS known from OS/2.
It is based on **NOM** the **N**etlabs **O**bject **M**odel. See <http://svn.netlabs.org/v_nom> for more information.

## About

The desktop is built from classes which live in shared libraries. 
Thanks to the underlying object system the user may add classes (whose instances
represent e.g. files or folders) without recompiling the desktop. Thus independend
vendors can provide **binary only** extensions to the desktop.

## How to contribute

Before implementing new stuff drop a note on the Voyager mailing list.
This prevents duplicate efforts if someone else is already working on the things you
have in mind. In addition ML discussions make sure any patches fit into the overall
architecture of the desktop.

### Licensing

Only patches and additions covered by the layout license for this project are accepted.
Compatible licenses are of course ok. No GPL code will be accepted in the main tree.

### Patch policy

Bug fixes may go directly into the tree. Enhancements of classes (like new methods) or
completely new classes will first have a test drive for some time to make sure they fit
in. Enhancements should be coded as replacement classes not as direct patches to existing
classes. If you need to patch an exisiting class to get something done it's likely
your design isn't optimal. This doesn't mean enhancement of existing classes to
support new features in subclasses are rejected. If it turns out your enhancements are
working flawlessly they will go into the main tree which means they will be added
directly to the classes in question.

### Documentation

Every new feature (e.g. methods) **must** be documented in the source using doxygen
tags. See the existing source for examples.

Comment your code thouroughly even if you think comments are not necessary because anyone
should be able to understand your code.

Patches without proper comments or doxygen documentation will be rejected without discussion.

### Coding style

**Function and method names**:

Names are not build with underscores like in the GLib toolkit. Instead uppercase letters
are used to separate name parts:

```c
dont_use_this_as_a_name();
```

```c
thisIsCorrect();
```

The first letter is always lowercase. Note that this is true for all functions and methods in NOM.[[BR]]
In pure C libraries the first letter is usually uppercase. 

**Variables**:

Use verbose variables. i, j, k was ok last century but today we have compilers supporting really long
variable names. Verbose names make it easier for newcomers to understand your code. Prepend your variables
with a meaningful marker (e.g. i for int).

```c
  int main(int argc, char *argv[])
    {
    /* This is not ok */
    gint i, k;
    char* p;

    printf("Hello World\n");
    ...
    }
```

```c
  int main(int argc, char *argv[])
    {
    /* Use something like this */
    gint iLoop, iNumberOfItems;
    gulong ulSize;
    char* pString;

    printf("Hello World\n");
    ...
    }
``` 

## Building it

See [this page](http://trac.netlabs.org/v_nom/wiki/BuildNom) for building it on OS/2.

See [this page](http://trac.netlabs.org/v_nom/wiki/BuildNomDarwin) for building it on Darwin (OS X).

See [this page](http://trac.netlabs.org/v_nom/wiki/BuildNomWindows) for building it on Windows.

See [this page](http://trac.netlabs.org/v_nom/wiki/BuildNomLinux) for building it on Linux.

