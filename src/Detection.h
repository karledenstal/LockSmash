#pragma once

class Detection {
public:
    Detection() = default;
    Detection(const Detection&) = delete;
    Detection(Detection&&) = delete;
    void operator=(const Detection&) = delete;
    void operator=(Detection&&) = delete;

    [[nodiscard]] static Detection* GetSingleton();

    void createDetection();
};
