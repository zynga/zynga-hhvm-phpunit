<?php
/*
 * This file is part of PHPUnit.
 *
 * (c) Sebastian Bergmann <sebastian@phpunit.de>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

use Zynga\PHPUnit\V2\Interfaces\TestInterface;
use Zynga\PHPUnit\V2\Interfaces\TestListenerInterface;
use \Exception;

/**
 * Base class for test listeners that interact with an issue tracker.
 *
 * @since Class available since Release 3.4.0
 */
abstract class PHPUnit_Extensions_TicketListener implements TestListenerInterface
{
    /**
     * @var array
     */
    protected $ticketCounts = [];

    /**
     * @var bool
     */
    protected $ran = false;

    /**
     * An error occurred.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addError(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * A failure occurred.
     *
     * @param TestInterface                 $test
     * @param Exception $e
     * @param float                                  $time
     */
    public function addFailure(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * Incomplete test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     */
    public function addIncompleteTest(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * Risky test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since Method available since Release 4.0.0
     */
    public function addRiskyTest(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * Skipped test.
     *
     * @param TestInterface $test
     * @param Exception              $e
     * @param float                  $time
     *
     * @since Method available since Release 3.0.0
     */
    public function addSkippedTest(TestInterface $test, Exception $e, $time)
    {
    }

    /**
     * A test suite started.
     *
     * @param TestInterface $suite
     *
     * @since Method available since Release 2.2.0
     */
    public function startTestSuite(TestInterface $suite)
    {
    }

    /**
     * A test suite ended.
     *
     * @param TestInterface $suite
     *
     * @since Method available since Release 2.2.0
     */
    public function endTestSuite(TestInterface $suite)
    {
    }

    /**
     * A test started.
     *
     * @param TestInterface $test
     */
    public function startTest(TestInterface $test)
    {
        if (!$test instanceof PHPUnit_Framework_WarningTestCase) {
            if ($this->ran) {
                return;
            }

            $name    = $test->getName(false);
            $tickets = PHPUnit_Util_Test::getTickets(get_class($test), $name);

            foreach ($tickets as $ticket) {
                $this->ticketCounts[$ticket][$name] = 1;
            }

            $this->ran = true;
        }
    }

    /**
     * A test ended.
     *
     * @param TestInterface $test
     * @param float                  $time
     */
    public function endTest(TestInterface $test, $time)
    {
        if (!$test instanceof PHPUnit_Framework_WarningTestCase) {
            if ($test->getStatus() == PHPUnit_Runner_BaseTestRunner::STATUS_PASSED) {
                $ifStatus   = ['assigned', 'new', 'reopened'];
                $newStatus  = 'closed';
                $message    = 'Automatically closed by PHPUnit (test passed).';
                $resolution = 'fixed';
                $cumulative = true;
            } elseif ($test->getStatus() == PHPUnit_Runner_BaseTestRunner::STATUS_FAILURE) {
                $ifStatus   = ['closed'];
                $newStatus  = 'reopened';
                $message    = 'Automatically reopened by PHPUnit (test failed).';
                $resolution = '';
                $cumulative = false;
            } else {
                return;
            }

            $name    = $test->getName(false);
            $tickets = PHPUnit_Util_Test::getTickets(get_class($test), $name);

            foreach ($tickets as $ticket) {
                // Remove this test from the totals (if it passed).
                if ($test->getStatus() == PHPUnit_Runner_BaseTestRunner::STATUS_PASSED) {
                    unset($this->ticketCounts[$ticket][$name]);
                }

                // Only close tickets if ALL referenced cases pass
                // but reopen tickets if a single test fails.
                if ($cumulative) {
                    // Determine number of to-pass tests:
                    if (count($this->ticketCounts[$ticket]) > 0) {
                        // There exist remaining test cases with this reference.
                        $adjustTicket = false;
                    } else {
                        // No remaining tickets, go ahead and adjust.
                        $adjustTicket = true;
                    }
                } else {
                    $adjustTicket = true;
                }

                $ticketInfo = $this->getTicketInfo($ticket);

                if ($adjustTicket && in_array($ticketInfo['status'], $ifStatus)) {
                    $this->updateTicket($ticket, $newStatus, $message, $resolution);
                }
            }
        }
    }

    /**
     * @param mixed $ticketId
     *
     * @return mixed
     */
    abstract protected function getTicketInfo($ticketId = null);

    /**
     * @param string $ticketId
     * @param string $newStatus
     * @param string $message
     * @param string $resolution
     */
    abstract protected function updateTicket($ticketId, $newStatus, $message, $resolution);
}
