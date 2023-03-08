Views Natural Sort
==================

Provides a views filter that sorts node titles by a more natural manner by ignoring articles like "The" and "A." This is done using a prebuilt, indexed table so it should perform well.

Requirements 
------------

This module requires that the following module is also enabled:

 * [Entity Plus](https://github.com/backdrop-contrib/entity_plus)

Installation and Usage
------------

- Install this module using the official Backdrop CMS instructions at
  https://docs.backdropcms.org/documentation/extend-with-modules.

- Also Enable Views Natural Sort Text Field support.

- Navigate to the fields edit page for the field that you wish to enable Natural Sorting on and check "Enable natural sorting." This is needed so that we aren't indexing other text fields needlessly bloating the sorting table. 
- When you do that, your fields "Should" be reindexed... at that time. You should be taken to a batch operations screen.
- Next you can go to the view you are editing and the option to sort the view naturally should be there. 

Issues 
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/foo-project/issues).

Current Maintainers <!-- This section is required. -->
-------------------

- [Paula Gray-Overtoom](https://github.com/pgrayove-mcpl).
- Seeking additional maintainers.

Credits <!-- This section is required. -->
-------

- Ported to Backdrop CMS by [Paula Gray-Overtoom](https://github.com/pgrayove-mcpl).
- Originally written for Drupal by [James Gilliland](https://www.drupal.org/u/neclimdul).
- Inital development sponsored by [Four Kitchens](https://www.drupal.org/four-kitchens).
- Drupal maintenance supported by [The University of British Columbia](https://www.drupal.org/the-university-of-british-columbia)

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

