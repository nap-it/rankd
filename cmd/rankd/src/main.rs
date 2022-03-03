mod capabilities;
mod config;
mod errors;
mod node;
mod options;
mod service;
//mod security;

use crate::config::{Log, Installation};
use failure::Error;
use slog::{info, debug, o, Drain, Duplicate};
use std::fs::OpenOptions;
use std::net::Ipv4Addr;
use std::path::PathBuf;
use std::str::FromStr;
use structopt::StructOpt;
use tokio::net::TcpListener;

#[tokio::main]
async fn main() {
    // Read options from CLI.
    let cli_options = options::RankdOpts::from_args();

    // Assign default values to CLI options.
    let mut log_file: PathBuf = PathBuf::from("/var/log/rankd.log");
    let mut quiet: bool = false;
    let mut server_ip: Ipv4Addr = Ipv4Addr::new(127, 0, 0, 1);
    let mut running_deployment_name: String = String::from("Default configuration");
    let mut installation_path: String = String::from("/etc/rank");

    match &cli_options.config_file {
        Some(config_file_path) => {
            let config = config::parse_file(config_file_path).unwrap();

            // Confirm title of the session.
            match config.title {
                None => {}
                Some(title) => {
                    running_deployment_name = title;
                }
            }

            // Get server options.
            server_ip = Ipv4Addr::from_str(config.server.ip.as_str()).unwrap();

            // Get logger options.
            match config.log {
                None => {}
                Some(log) => {
                    log_file = PathBuf::from(log.file);
                }
            }

            // Get installation options.
            match config.installation {
                None => {}
                Some(installation) => {
                    installation_path = installation.path;
                }
            }
        }
        None => {}
    }

    match &cli_options.log_file {
        Some(log_file_path) => {
            log_file = log_file_path.to_path_buf();
        }
        None => {}
    }

    // Create or verify the creation of keys. TODO Security was dropped off.
    //security::create_identifier(&installation_path, false);

    // Initialize logger.
    let log_file = OpenOptions::new()
        .create(true)
        .write(true)
        .truncate(true)
        .open(log_file)
        .unwrap();
    let terminal_decorator = slog_term::TermDecorator::new().build();
    let terminal_drain = slog_term::FullFormat::new(terminal_decorator)
        .build()
        .fuse();
    let terminal_drain = slog_async::Async::new(terminal_drain)
        .build()
        .filter_level(match &cli_options.verbose {
            0 => slog::Level::Info,
            _ => slog::Level::Debug,
        })
        .fuse();
    let file_decorator = slog_term::PlainDecorator::new(log_file);
    let file_drain = slog_term::FullFormat::new(file_decorator)
        .build()
        .filter_level(slog::Level::Debug)
        .fuse();
    let file_drain = slog_async::Async::new(file_drain).build().fuse();
    let log = if !cli_options.quiet {
        slog::Logger::root(Duplicate::new(terminal_drain, file_drain).fuse(), o!())
    } else {
        slog::Logger::root(file_drain, o!())
    };

    info!(log, "rankd is now starting its server.");

    service::start(log, server_ip, installation_path.clone()).await;
}
