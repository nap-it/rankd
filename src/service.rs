use warp::Filter;
use slog::{Logger, info};

pub async fn start(log: Logger) {
    let testing_tool = warp::path::end().map(move || {
        info!(log, "Received a GET request on /.");
        format!("Welcome to this service")
    });

    let routes = warp::get().and(
        testing_tool
    );

    warp::serve(routes).run(([127, 0, 0, 1], 7265)).await;
}