#ifndef CONVERT_PIP_HPP
#define CONVERT_PIP_HPP

std::string convert(const std::string& data)
{
    std::string newData(data);
    for (auto &letter: newData)
    {
        if (letter >= 97 && letter <= 102)
        {
            letter -= 32;
        }
    }

    for (auto &letter: newData)
    {
       if (letter >= 65 && letter <= 70)
       {
           letter -= 17;
       }
    }
    return newData;
}

#endif // CONVERT_PIP_HPP
