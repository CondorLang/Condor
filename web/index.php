<?php

require('../vendor/autoload.php');

$app = new Silex\Application();

use Symfony\Component\HttpFoundation\BinaryFileResponse;
use Symfony\Component\HttpFoundation\ResponseHeaderBag;

$app['debug'] = true;

// Register the monolog logging service
$app->register(new Silex\Provider\MonologServiceProvider(), array(
  'monolog.logfile' => 'php://stderr',
));

// Register view rendering
$app->register(new Silex\Provider\TwigServiceProvider(), array(
    'twig.path' => __DIR__.'/views',
));

// Our web handlers

$app->get('/', function() use($app) {
  $app['monolog']->addDebug('logging output.');
  return $app['twig']->render('index.twig');
});

$app->get('/build/{platform}', function($platform) use($app){
	// $app['monolog']->addDebug('logging output.');
	// return file_get_contents();

  $filePath = '../build/' . strtolower($platform) . '/Condor';

  // prepare BinaryFileResponse
  $response = new BinaryFileResponse($filePath);
  $response->trustXSendfileTypeHeader();
  $response->setContentDisposition(
      ResponseHeaderBag::DISPOSITION_INLINE,
      $filename,
      iconv('UTF-8', 'ASCII//TRANSLIT', $filename)
  );

  return $response;
});

$app->run();
