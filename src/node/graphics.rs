use nvml_wrapper::error::NvmlError;
use nvml_wrapper::enum_wrappers::device::{Brand, Clock, EncoderType, OperationMode, TemperatureSensor};
use nvml_wrapper::enum_wrappers::device::ClockId::Current;

#[derive(Debug, Serialize, Deserialize)]
pub struct Graphics<'a> {
    _device: nvml_wrapper::device::Device<'a>,
    brand: String,
    clock_graphics: u32,
    clock_sm: u32,
    clock_memory: u32,
    clock_video: u32,
    processes_running: u32, // running_compute_processes_count
    is_display_active: bool,
    is_display_connected: bool,
    encoder_capacity_h264: u32,
    encoder_capacity_hevc: u32,
    encoder_session_count: u32,
    encoder_average_fps: u32,
    encoder_average_latency: u32,
    power_limit: u32,
    fan_speed: u32,         // average of vector
    operation_mode: String, // OperationMode
    info_rom_version: String,
    clock_max_graphics: u32,
    clock_max_sm: u32,
    clock_max_memory: u32,
    clock_max_video: u32,
    memory_free: u64,
    memory_used: u64,
    memory_total: u64,
    name: String,
    power_usage: u32,
    serial_no: String,
    board_part_no: String,
    temperature: u32,
    uuid: String,
}

impl Graphics {
    pub fn there_is_a_graphics_card() -> bool {
        match nvml_wrapper::NVML::init().unwrap().device_by_index(0) {
            Ok(_) => true,
            Err(_) => false,
        }
    }

    pub fn load() -> Option<Self> {
        if !Self::there_is_a_graphics_card() {
            return None;
        }

        let device = nvml_wrapper::NVML::init()
            .unwrap()
            .device_by_index(0)
            .unwrap();
        Some(Graphics {
            _device: device,
            brand: match device.brand().unwrap() {
                Brand::Unknown => String::from("Unknown"),
                Brand::Quadro => String::from("Quadro"),
                Brand::Tesla => String::from("Tesla"),
                Brand::NVS => String::from("NVS"),
                Brand::GRID => String::from("GRID"),
                Brand::GeForce => String::from("GeForce"),
                Brand::Titan => String::from("Titan"),
            },
            clock_graphics: device.clock(Clock::Graphics, Current).unwrap(),
            clock_sm: device.clock(Clock::SM, Current).unwrap(),
            clock_memory: device.clock(Clock::Memory, Current).unwrap(),
            clock_video: device.clock(Clock::Video, Current).unwrap(),
            processes_running: device.running_compute_processes_count().unwrap(),
            is_display_active: device.is_display_active().unwrap(),
            is_display_connected: device.is_display_connected().unwrap(),
            encoder_capacity_h264: device.encoder_capacity(EncoderType::H264).unwrap(),
            encoder_capacity_hevc: device.encoder_capacity(EncoderType::HEVC).unwrap(),
            encoder_session_count: device.encoder_stats().unwrap().session_count,
            encoder_average_fps: device.encoder_stats().unwrap().average_fps,
            encoder_average_latency: device.encoder_stats().unwrap().average_latency,
            power_limit: device.power_management_limit().unwrap(),
            fan_speed: device.fan_speed(0).unwrap(),
            operation_mode: match device.gpu_operation_mode().unwrap().current {
                OperationMode::AllOn => String::from("All On"),
                OperationMode::Compute => String::from("Compute"),
                OperationMode::LowDP => String::from("Low DP"),
            },
            info_rom_version: device.info_rom_image_version().unwrap(),
            clock_max_graphics: device.max_clock_info(Clock::Graphics).unwrap(),
            clock_max_sm: device.max_clock_info(Clock::SM).unwrap(),
            clock_max_memory: device.max_clock_info(Clock::Memory).unwrap(),
            clock_max_video: device.max_clock_info(Clock::Video).unwrap(),
            memory_free: device.memory_info().unwrap().free,
            memory_used: device.memory_info().unwrap().used,
            memory_total: device.memory_info().unwrap().total,
            name: device.name().unwrap(),
            power_usage: device.power_usage().unwrap(),
            serial_no: device.serial().unwrap(),
            board_part_no: device.board_part_number().unwrap(),
            temperature: device.temperature(TemperatureSensor::Gpu).unwrap(),
            uuid: device.uuid().unwrap(),
        })
    }
}
