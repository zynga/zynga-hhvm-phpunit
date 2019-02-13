<?hh // strict

/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Zynga\PHPUnit\V2\Tests\Mock;

use Zynga\PHPUnit\V2\Tests\Mock\Book;

/**
 * An author.
 *
 * @since      Class available since Release 3.6.0
 */
class Author {
  // the order of properties is important for testing the cycle!
  public Vector<Book> $books = Vector {};

  private string $name = '';

  public function __construct(string $name) {
    $this->name = $name;
  }
}
